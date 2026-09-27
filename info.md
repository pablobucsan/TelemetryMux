**Mini Packet Router / Telemetry Mux**  
A small, self-contained C program whose only purpose is deliberate practice of bit manipulation and concurrency. It is not connected to MiniFlight.

### Purpose
Simulate a tiny concurrent packet pipeline:
- Multiple producer threads generate fixed-size packets that contain tightly packed bit-fields.
- Packets travel through a shared fixed-size ring buffer.
- Consumer threads pull packets, decode the bit-fields, validate them, update shared statistics and a packed status word, then release the slot.
- Optional extra modules force additional concurrency patterns (multiple locks, reader-writer, atomics, clean shutdown).

The program should run indefinitely (or for a configurable number of packets) and print periodic status so you can observe correctness under load.

### Core data

**Packet**  
Fixed-size blob, recommended 16 or 32 bytes. Treat it as an array of `uint8_t` (or overlay `uint32_t`/`uint64_t` views for convenience).  
Logical fields living inside the bytes (exact bit positions are part of the design you freeze before coding):

- Magic / version (a few bits or a byte that must match a constant).  
- Source / producer ID (small integer).  
- Sequence number (16- or 32-bit, wraps).  
- Flags / status bits (individual bits or small bit-fields: e.g. “priority”, “error”, “last-of-group”).  
- Payload value(s) (one or two small integers of varying bit widths).  
- Reserved bits (must be zero on a valid packet).  
- Parity or simple checksum covering selected fields (1–8 bits).

All multi-bit fields are packed with explicit shifts and masks; no reliance on compiler bit-fields for the core practice.

**Ring buffer**  
Fixed capacity (e.g. 16 or 32 slots). Each slot holds one packet.  
Classic circular buffer with head and tail indices (or a count).  
Slots are owned by either a producer (while filling) or a consumer (while processing); ownership transfer happens only under protection.

**Shared status / statistics**  
A small structure (or a single packed word plus a few counters) visible to all threads:
- Total packets processed.
- Validation failure counts (broken out by reason if useful).
- Sticky error flags (bit-packed).
- Last sequence number seen, or similar.
- Optionally a generation / version counter for reader-writer practice.

### Threads and responsibilities

**Producers (start with 2, later 3+)**  
Loop:
1. Obtain a free slot from the ring (block or back off if full).
2. Fill the packet bytes: set magic/version, own ID, next sequence number, chosen flags, payload values, reserved = 0, compute parity/checksum.
3. Make the slot visible to consumers (publish).
4. Optionally sleep a short random time or spin to vary timing.

**Consumers (start with 1, later 2)**  
Loop:
1. Obtain a filled slot (block if empty).
2. Decode every field with shifts and masks.
3. Validate (see below).
4. Update shared statistics and status word.
5. Release the slot back to the free pool.
6. Optionally produce a tiny “reply” or just discard.

**Monitor (optional but useful)**  
A low-frequency thread that periodically reads the shared status (under the appropriate protection) and prints a human-readable summary. This makes races and correctness visible without attaching a debugger.

### Packet validation (bit-manipulation focus)
Every consumer must reject a packet that fails any of these checks:
- Magic / version mismatch.
- Reserved bits not zero.
- Source ID out of allowed range.
- Sequence number anomaly (optional: detect large jumps or duplicates if you track last-seen).
- Parity or checksum mismatch.
- Cross-field rules (example: if a particular flag is set, a payload field must be non-zero; or length-like field must be consistent with other bits).

On failure: increment the appropriate error counter, set a sticky bit in the status word, and still release the slot (do not leak). On success: increment the good-packet counter and clear or leave sticky bits as designed.

### Concurrency surface (what you will implement and be able to discuss)

**Baseline (always present)**  
- Mutex protecting the ring-buffer indices / count.  
- Condition variables (or equivalent) for “not full” and “not empty”.  
- Clear ownership rules for each slot.  
- Safe update of the shared statistics / status word.

**Optional modules (add one at a time)**  
1. Second independent shared structure with its own mutex → forces lock-ordering discipline and deadlock awareness.  
2. Reader-writer access to the status word (consumers write, monitor and maybe producers read) → `pthread_rwlock` or a simple sequence-counter technique.  
3. Hot counter or flag updated with C11 atomics (`_Atomic`) so you can contrast mutex vs. atomic and speak about memory ordering at a high level.  
4. Clean shutdown: a global “running” flag, producers stop submitting, consumers drain the queue, all threads join. Surfaces lifetime and “who frees what” questions.

You should be able to turn any of these modules on or off with a compile-time or run-time switch so the program stays understandable.

### Configuration and observability
- Command-line or compile-time knobs: number of producers, number of consumers, ring capacity, total packets (or run forever), optional module flags.  
- Periodic status print from the monitor (or from a consumer every N packets).  
- At exit: final counts and a simple consistency check (packets produced == packets consumed + still-in-queue, error counts non-negative, etc.).

### Success criteria
- Under load (multiple producers + consumers, short sleeps or none) the program runs for minutes without data races, lost packets, double-processed packets, or corrupted status.  
- Validation correctly rejects deliberately malformed packets that producers occasionally inject.  
- You can explain every critical section, every ownership transfer, and every bit operation.  
- You can discuss the trade-offs of each optional module (clarity vs. performance, deadlock risk, memory-ordering complexity, etc.).

### Suggested implementation order
1. Packet layout + pure bit encode/decode/validate functions (single-threaded test).  
2. Ring buffer + single producer / single consumer with mutex + condvars.  
3. Multiple producers / consumers.  
4. Shared status word and validation counters.  
5. One optional module at a time.  
6. Stress and deliberate fault injection (malformed packets, high contention).

### Non-goals
- No real I/O, networking, or hardware.  
- No dynamic allocation in the steady-state path (fixed pools only).  
- No connection to the MiniFlight codebase.  
- Keep total size modest (a few hundred lines).

This description is intentionally complete enough that you could sit down and implement from it. The exact bit positions, field widths, and which optional modules you enable first are the next decisions we can freeze together if you want to start coding.
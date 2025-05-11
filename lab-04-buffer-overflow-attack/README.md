# Lab 4: Buffer Overflow Attack

## Overview

In **Lab 4**, I performed a **buffer overflow attack** against a vulnerable C function on ESP32. The lab demonstrates memory exploitation techniques by crafting a custom payload that hijacks the return address to enable `adminMode`.

## Key Features

- Locates vulnerable function and calculates stack frame layout
- Identifies return address and crafts custom payload
- Demonstrates successful hijacking of execution flow
- Provides mitigation using `strncpy`
- Built with **ESP-IDF**, compiled for **ESP32-C6**

## Task Description

The task description can be found in the [task-description.md](task-description.md) file.

## Solution

This lab includes:

- `buffer_overflow/`: ESP-IDF project with the vulnerable function and crafted exploit
- `vulnerable.c`: Original vulnerable source file
- `buffer_overflow.c`: Modified solution that launches the attack
- `attack-walkthrough.md`: Step-by-step buffer overflow attack execution
- `attack-buffer-details.md`: Deep dive into buffer structure and RISC-V shellcode

## Tools Used

- **ESP-IDF**: For building and flashing ESP32 apps
- **GDB**: For debugging stack and register states
- **objdump**: To inspect RISC-V machine code
- **ESP32-C6**: Target device for executing the attack

## How to Run

1. Build and flash the `buffer_overflow` project with your payload.
2. Use `idf.py monitor` to observe program output.
3. You should see the `ADMIN MODE!!!` message upon successful overflow.
4. To verify the fix, switch to `vulnerable.c` with `strncpy()` patch and rebuild.

---

## Attack Steps

### Step 1: Determine the Stack Pointer (SP) before calling the vulnerable function

To calculate the layout of the stack, we first need to know the top of the stack before entering the vulnerable function.

- Set a breakpoint before the call to `vulnerableMQTTHandler`.
- Run the debugger.
- In the Debug Console, enter:

```bash
>p $sp
```

- Result:

```
$1 = (void *) 0x408104b0
```

- Explanation:
  - This is the top of the stack before calling `vulnerableMQTTHandler`.
  - It is the upper boundary of the new stack frame.
  - The stack grows downward, so the frame will be below this address.

---

### Step 2: Determine the Address of `adminMode`

We need the address of `adminMode`, since our goal is to change its value through the overflow.

- In the Debug Console, enter:

```bash
>p &adminMode
```

- Result:

```
$2 = 0x4080d4e8 <adminMode>
```

- Explanation:
  - This address will be used as part of the payload to activate admin privileges.

---

### Step 3: Determine the Stack Pointer (SP) inside the vulnerable function

After entering `vulnerableMQTTHandler`, we need to find the new value of the stack pointer.

- Set a breakpoint inside the function (e.g., on the `strcpy` line).
- Run the debugger.
- In the Debug Console, enter:

```bash
>p $sp
```

- Result:

```
$3 = (void *) 0x40810470
```

- Explanation:
  - This is the bottom of the stack frame used by `vulnerableMQTTHandler`.
  - The frame spans from this address up to the previously found `0x408104b0`.

---

### Step 4: Determine the Size of the Stack Frame

We now check how much stack space the function allocates.

- In the Debug Console, enter:

```bash
>disassemble vulnerableMQTTHandler
```

- Look for the first instruction:

```
0x4200b6fe <+0>: addi sp, sp, -64
```

- Explanation:
  - This instruction reserves 64 bytes on the stack.
  - This instruction tells us that 64 bytes are allocated for the stack frame.
  - We will use this information in the next steps to calculate the actual frame boundaries.

---

### Step 5: Find the Return Address Value Stored in the Stack

We need to know what value gets stored as the return address when the function is called.

- In the Debug Console, enter:

```bash
>disassemble app_main
```

- Find the call to the vulnerable function:

```
0x4200b784 <+46>: jal 0x4200b6fe <vulnerableMQTTHandler>
0x4200b788 <+50>: lui a5, 0x4080d
```

- Explanation:
  - `jal` saves the address of the next instruction (`0x4200b788`) into the return address register (`ra`).
  - This value is then saved into the stack and can be overwritten by our payload.

---

### Step 6: Define the Exact Stack Frame Boundaries

Now that we know the start of the stack frame and its size, we can calculate the exact memory range used by the function.

- Stack pointer at function entry: `0x40810470`
- Stack frame size: 64 bytes

**Calculation:**

```
0x40810470 + 64 = 0x408104b0
```

- This means the stack frame occupies the range:

```
0x40810470 → 0x408104af
```

(This is calculated as: 0x40810470 + 64 (bytes) - 1 (byte) = 0x408104af)

> Note: The `64` byte size comes from the previous step and is directly related to the stack frame size.

> So the vulnerable function’s stack frame is between `0x40810470` and `0x408104af`

- The address `0x408104b0` is just outside the frame and marks the top of the stack before the function call.

### Step 7: Sample Output When the Attack Succeeds

When the payload successfully overwrites the return address and execution jumps to the target function (e.g., `setAdminMode()`), we should see output similar to this in the console:

```
Hello world!
Processed MQTT Message: ����@G#��N��@�� ���7��^�����
ADMIN MODE!!!
```

- The `ADMIN MODE!!!` message is printed in `app_main`, but it is only triggered if `setAdminMode()` sets the `adminMode` flag to 1.
- This confirms that our buffer overflow attack worked, and we successfully hijacked the control flow, redirecting it to the target function.

---

## Fix the Vulnerability

After confirming the buffer overflow vulnerability, we modified the source code to make it safe.

- In the `vulnerableMQTTHandler` function (inside `vulnerable.c`), we replaced:

```c
strcpy(smallbuffer, message);
```

- With a safer version using `strncpy`:

```c
strncpy(smallbuffer, message, sizeof(smallbuffer) - 1);
smallbuffer[sizeof(smallbuffer) - 1] = '\0';
```

**Explanation:**

- This change limits the number of copied bytes to prevent overflow.
- It also ensures the buffer is null-terminated.
- This fix prevents potential attacks by ensuring that we never copy more data than the buffer can handle. By using `strncpy` and controlling the number of copied bytes, we guarantee that no buffer overflow occurs, keeping the stack safe.

## Appendix

See [attack-buffer-details.md](attack-buffer-details.md) for a full walkthrough on buffer crafting and assembly-level optimization.

See [attack-walkthrough.md](attack-walkthrough.md) for a detailed step-by-step attack guide including payload construction and analysis.

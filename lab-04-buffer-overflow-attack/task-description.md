# Lab 4: Buffer Overflow Attack

This lab demonstrates how a buffer overflow vulnerability can be exploited to gain unauthorized access, and how to fix it using secure coding techniques.

## 4.1 Demonstrate the attack

1. Set up a project and add vulnerable.c as the project source (can start with helloworld example).
2. Build and test. You should see test messages printed.
3. Follow the steps from [attack-walkthrough.md](attack-walkthrough.md), adapting them to your memory layout if necessary. Make sure to check all relevant addresses during preparation and document the following:
   - address of the adminMode variable
   - top of stack address when in vulnerable function
     - If your top of stack happens to end with 0x00, you need to modify the setup to avoid that address. The easiest solution is to move attack code 4 addresses forward in memory, so that the start address ends with 0x04 == no NULLs. Do that by moving one line of padding in new place.
   - return address within the app_main function
4. Capture and include the terminal output showing `ADMIN MODE!!!` to confirm the attack was successful.

## 4.2 Fix the vulnerability

Locate the vulnerable code in [vulnerable.c](vulnerable.c), apply an appropriate fix to eliminate the buffer overflow risk, and commit the updated project to your repository.

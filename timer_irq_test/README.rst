.. zephyr:code-sample:: Timer IRQ Test
   :name: Read clock test

   Simple tests that reads the hardware clock

Read Clock Test
###########

Overview
********
~ 

Building and Running
********************

This application can be built and executed on QEMU as follows:

.. zephyr-app-commands::
   :zephyr-app: /timer_irq_test
   :host-os: unix
   :board: stm32f4_disco
   :goals: run
   :compact:

Sample Output
=============

.. code-block:: console

   *** Booting Zephyr OS build v3.7.0-2913-gd41e834bbe2f ***
   Main starts
   0:Total: 0 s. 000463875 ns N:100 Off: 0 s. 000004619 ns Avg: = 0 s. 000000023ns 
   1000:Total: 0 s. 000464125 ns N:100 Off: 0 s. 000004619 ns Avg: = 0 s. 000000023ns 
   2000:Total: 0 s. 000464148 ns N:100 Off: 0 s. 000004619 ns Avg: = 0 s. 000000023ns 
   3000:Total: 0 s. 000464165 ns N:100 Off: 0 s. 000004619 ns Avg: = 0 s. 000000023ns 
   4000:Total: 0 s. 000464139 ns N:100 Off: 0 s. 000004619 ns Avg: = 0 s. 000000023ns 
   5000:Total: 0 s. 000464179 ns N:100 Off: 0 s. 000004619 ns Avg: = 0 s. 000000023ns 
   6000:Total: 0 s. 000464182 ns N:100 Off: 0 s. 000004619 ns Avg: = 0 s. 000000023ns 
   7000:Total: 0 s. 000464188 ns N:100 Off: 0 s. 000004619 ns Avg: = 0 s. 000000023ns 
   8000:Total: 0 s. 000464204 ns N:100 Off: 0 s. 000004619 ns Avg: = 0 s. 000000023ns 
   9000:Total: 0 s. 000464118 ns N:100 Off: 0 s. 000004619 ns Avg: = 0 s. 000000023ns 

   Main ends




# SAMD Binary Counter

PCB by:  Bryce Hill</br>
Code By: Miller DeMark

## Overview

This repository contains the code for the SAMD Binary Counter created for Montana Tech's Electrical Engineering Department for student outreach. The counter displays a count from 0-1024 on a Bar LED, and can be communicated with via Serial in a terminal over USB. The following code has comments explaining how it works, and there are documents going into further depth on how to adjust code as needed or if changes will need to be made in the future.

 ## Table of Contents

- [Atmel](/docs/atmel.md)
    - [Opening Project](/docs/atmel.md#gettng-started)
    - [Navigating The Project](/docs/atmel.md#navigating)
    - [Reading and Revising Code](/docs/atmel.md#code)
    - [Code Breakdown](/docs/atmel/#breakdown)
- [Firmware](/docs/firmware.md)
    - [Obtaining .hex file](/docs/firmware.md#the-hex-file)
- [Flashing the Counter](/docs/counterflash.md)
    - [What You'll Need](/docs/counterflash.md#what-you'll-need)
    - [Python Tool](/docs/counnterflash.md#python-tool)
    - [Testing Board](/docs/counterflash.md#testing-board)
    - [Running the Flash Tool](/docs/counterflash.md#running-the-flash-tool)
- [Finishing Touches](/docs/finishingtouches.md)
    - [The LED Bar](/docs/finishingtouches.md#led-bar)
    - [USB and Button Solders](/docs/finishingtouches.md#usb-and-button-solders)
- [Notes](/docs/notes.md)

## Other Relevant Repositories

To porperly make these counters for the Electrical Engineering Department, you'll be using the Pick and Place Machine and most likely the Inventory Server to track components. Those repos are linked here as reference guides to get up and running quickly.

- [Pick and Place](github.com/mdemark18/pick-and-place)
- [Inventory Server](github.com/mdemark18/inventory-server)
# Springer Silentia IR remote control

## Description

Project for using a arduino board as a remote control for Springer silentia air conditioner models using IRremote arduino library.

## Components

- Arduino Uno Board
- Infrared LED and resistor (connected to pin 3)
- Infrared receiver (connected to pin 2)

<img align="center" width="700" alt="Arduino" src="Arduino.jpg">

## How it works

It works by periodicly sending a pulse width signal stored in its memory. The signal sent can be changed by using a NEC protocol IR remote to 


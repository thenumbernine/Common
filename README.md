### Common classes other projects use

[![Donate via Stripe](https://img.shields.io/badge/Donate-Stripe-green.svg)](https://buy.stripe.com/00gbJZ0OdcNs9zi288)<br>
[![Donate via Bitcoin](https://img.shields.io/badge/Donate-Bitcoin-green.svg)](bitcoin:37fsp7qQKU8XoHZGRQvVzQVP8FrEJ73cSJ)<br>

This repo also contains the base Makefile for all my other C++ projects.

This also builds with my own build system, [lmake](https://github.com/thenumbernine/lua-make).

How to build with make:

	make <platform>

or edit Config.mk and choose your appropriate platform, then it's just:

	make

...to build debug and release builds.
also allowed:

	make debug
		or
	make <platform>_debug
	
	make release
		or
	make <platform>_release

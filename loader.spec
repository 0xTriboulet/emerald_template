#
# Simple Reflective DLL build spec
#
x86:

	# load our x86 .o file AND turn it into position-independent code
	# CHANGE THIS TO YOUR DLL
    load "bin/loader.x86.o"
		# +gofirst moves go() to position 0 of our PIC
		make pic +gofirst

		# OPT into x86 program fixes to allow data references without code hacks
		fixptrs "_caller"

		# OPT into PIC dynamic function resolution
		dfr "_resolve" "ror13"

		# merge the Tradecraft Garden Library into our PIC
		mergelib "tcg/libtcg/libtcg.x86.zip"

		# load our Reflective DLL argument AND link it into our PIC as my_data section
		push "crystal-palace/demo/test.x86.dll"
			link "my_data"

	    # use cp-dfr-specs to auto-convert APIs to DFR
        run "deps/dfr_windows.spec"

		# we're done, export the final blob
		export

x64:

	load "bin/loader.x64.o"
		make pic +gofirst

		dfr "resolve" "ror13"
		mergelib "tcg/libtcg/libtcg.x64.zip"

        # CHANGE THIS TO YOUR DLL
		load "crystal-palace/demo/test.x64.dll"
			link "my_data"

        # use cp-dfr-specs to auto-convert APIs to DFR
        run "deps/dfr_windows.spec"

		export

# TODO(Nowar): Take out per each and build it.
$(foreach gdk_module, $(ALL_BITCODE_MODULES),\
  $(info Android GDK: Compile $(BITCODE_MODULE.$(gdk_module).SRC_FILES) of LOCAL_MODULE $(gdk_module))\
)

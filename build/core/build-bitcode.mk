.PHONY: $(LOCAL_MODULE)
ALL_BITCODE_MODULES += $(LOCAL_MODULE)

# From now on, we only support these flags.
BITCODE_MODULE.$(LOCAL_MODULE).PATH        := $(LOCAL_PATH)
BITCODE_MODULE.$(LOCAL_MODULE).SRC_FILES   := $(LOCAL_SRC_FILES)
BITCODE_MODULE.$(LOCAL_MODULE).CFLAGS      := $(LOCAL_CFLAGS)
BITCODE_MODULE.$(LOCAL_MODULE).CPPFLAGS    := $(LOCAL_CFLAGS) $(LOCAL_CPPFLAGS)

$(foreach include_dir,$(LOCAL_C_INCLUDES),\
  $(eval BITCODE_MODULE.$(LOCAL_MODULE).CFLAGS += -I$(LOCAL_PATH)/$(include_dir))\
)
$(foreach include_dir,$(BITCODE_C_INCLUDES),\
  $(eval BITCODE_MODULE.$(LOCAL_MODULE).CFLAGS += -I$(include_dir))\
)

$(foreach include_dir,$(LOCAL_C_INCLUDES),\
  $(eval BITCODE_MODULE.$(LOCAL_MODULE).CPPFLAGS += -I$(LOCAL_PATH)/$(include_dir))\
)
$(foreach include_dir,$(BITCODE_C_INCLUDES),\
  $(eval BITCODE_MODULE.$(LOCAL_MODULE).CPPFLAGS += -I$(include_dir))\
)

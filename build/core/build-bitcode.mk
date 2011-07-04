.PHONY: $(LOCAL_MODULE)

$(foreach src,$(filter %c,$(LOCAL_SRC_FILES)),$(call compile-c-to-bc,$(src)))


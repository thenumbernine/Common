# anyone that needs Common should include this
# note that Base.mk should already be included, so this can use COMMON_PATH 
DYNAMIC_LIBS+=$(COMMON_PATH)dist/$(PLATFORM)/$(BUILD)/libCommon$(LIB_SUFFIX)

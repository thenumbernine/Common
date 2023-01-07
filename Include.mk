# anyone that needs Common should include this
# note that Base.mk should already be included, so this can use COMMON_PATH 
DEPEND_LIBS+=$(COMMON_PATH)dist/$(PLATFORM)/$(BUILD)/$(LIB_PREFIX)Common$(LIB_SUFFIX)

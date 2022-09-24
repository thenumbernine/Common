# anyone that needs Common should include this
# note that Base.mk should already be included, so this can use COMMON_PATH 
# TODO instead copy them into the dist folder
DYNAMIC_LIBS+=$(COMMON_PATH)dist/$(PLATFORM)/$(BUILD)/libCommon$(LIB_SUFFIX)

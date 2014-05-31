# anyone that needs Common should include this
# note that Base.mk should already be included, so this can use COMMON_PATH 
LIBS+=Common
LIBPATHS+=$(COMMON_PATH)dist/$(PLATFORM)/$(BUILD)

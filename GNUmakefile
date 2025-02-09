ifndef SKOFL_ROOT
	$(error SKOFL_ROOT is not set)
endif

include $(SKOFL_ROOT)/config.gmk

read_zbs: read_zbs.o
	LD_RUN_PATH=$(LIBDIR):$(ATMPD_SRC)/include/:$(SKOFL_LIBDIR):$(LOCAL_INC) $(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)


clean: 
	$(RM) *.o *~ core fort.* *.root *.zbs *.log 
	$(RM) read_zbs

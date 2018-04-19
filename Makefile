.PHONY: clean
CXX = g++
INCS = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux

libhierarchy.so: agent.cpp
	$(CXX) $(INCS) -shared -fPIC -O2 $^ -o $@

clean:
	rm -f libhierarchy.so *.class

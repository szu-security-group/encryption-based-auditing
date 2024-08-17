CXX = g++
CXXFLAGS = -std=c++17 -Wall -msse4.1 -maes -mpclmul -msse2
LDFLAGS = -lssl -lcrypto -lcryptopp


SRCS = Client.cpp  Server.cpp  general.cpp  gfmul.cpp  main.cpp  timer.cpp


TARGET = run
STATIC_TARGET = static_run



$(TARGET): $(SRCS)
	$(CXX) -O3 $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

$(STATIC_TARGET): $(SRCS)
	$(CXX) -O3 $(CXXFLAGS) -o $(STATIC_TARGET) $(SRCS) -static $(LDFLAGS) 

clean:
	rm -f $(TARGET) $(STATIC_TARGET)


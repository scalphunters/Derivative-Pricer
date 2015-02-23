 
.SUFFIXES : .cpp .o
CC = g++
INCS= 
LIBS = -ldl -lboost_date_time -ltinyxml -lmysqlclient
SRCS = main.cpp marketdata.cpp convention.cpp instrument.cpp curve.cpp utilities.cpp io.cpp parser.cpp
OBJS = main.o marketdata.o convention.o instrument.o curve.o utilities.o io.o parser.o
TARGET = main.exe

$(TARGET) :$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)
$(OBJS):$(SRCS)
	$(CC) -c $(SRCS) $(INCS)
compile:
	$(CC) -c $(SRCS) $(INCS)
link:
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)
clean:
	rm -rf $(OBJS) $(TARGET) core
new :
	$(MAKE) clean
	$(MAKE)
run :
	$(MAKE) new
	./$(TARGET)
run2 :
	$(MAKE)
	./$(TARGET)

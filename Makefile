OBJECTS = cmd_srv.o cmd_set.o cmd_get.o cmd_other.o
LIBS = -lpthread -lrt

all: cmd_srv

cmd_srv.o: cmd_srv.c $(HEADERS)
	$(CC) -c $< -fPIC -O2 -o $@

cmd_set.o: cmd_set.c $(HEADERS)
	$(CC) -c $< -fPIC -O2 -o $@

cmd_get.o: cmd_get.c $(HEADERS)
	$(CC) -c $< -fPIC -O2 -o $@

cmd_other.o: cmd_other.c $(HEADERS)
	$(CC) -c $< -fPIC -O2 -o $@

cmd_srv: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -fPIC -O2 -o $@
	$(STRIP) $@

.PHONY: clean

clean:
	rm -f cmd_srv
	rm -f $(OBJECTS)

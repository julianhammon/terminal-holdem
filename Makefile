SRCDIR = src
OBJDIR = obj
BINDIR = bin

$(BINDIR)/terminal-holdem: $(OBJDIR)/main.o
	gcc -o $@ $^

$(OBJDIR)/main.o: $(SRCDIR)/main.c
	mkdir -p obj
	mkdir -p bin
	gcc -c -o $@ $^

$(BINDIR)/card-preview: $(OBJDIR)/card-preview.o
	gcc -o $@ $^

$(OBJDIR)/card-preview.o: $(SRCDIR)/card-preview.c
	mkdir -p obj
	mkdir -p bin
	gcc -c -o $@ $^

clean:
	rm -f $(OBJDIR)/* $(BINDIR)/*
EXE:= main
INC_DIR:= ./include
SRC_DIR:= ./src
SRCS:= $(wildcard $(SRC_DIR)/*.cc)
LIBS:= -llog4cpp
CXXFLAGS:= -g

$(EXE):$(SRCS)
	g++ -o $@ $^  $(LIBS) $(addprefix -I, $(INC_DIR)) $(CXXFLAGS)
clean:
	rm main

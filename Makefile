CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -Wpedantic -O2
LDFLAGS := -pthread

ifeq ($(OS),Windows_NT)
	EXE := .exe
	STATIC_FLAGS := -static
else
	EXE :=
	STATIC_FLAGS :=
endif

SISTEMA_DIR := sistema_completo
SISTEMA_SRCS := \
	$(SISTEMA_DIR)/src/FSM_Controlador.cpp \
	$(SISTEMA_DIR)/src/Veiculo.cpp \
	$(SISTEMA_DIR)/src/Carro.cpp \
	$(SISTEMA_DIR)/src/Moto.cpp \
	$(SISTEMA_DIR)/src/Caminhao.cpp \
	$(SISTEMA_DIR)/src/RelogioCalendario.cpp \
	$(SISTEMA_DIR)/src/Estacionamento.cpp \
	$(SISTEMA_DIR)/src/main.cpp

.PHONY: all modulo1 modulo2 modulo3 sistema test clean

all: modulo1 modulo2 modulo3 sistema

modulo1:
	$(CXX) $(CXXFLAGS) $(STATIC_FLAGS) \
		modulos/01_fsm_cancela/src/FSM_Controlador.cpp \
		modulos/01_fsm_cancela/src/main_teste.cpp \
		-o modulos/01_fsm_cancela/bin/teste_fsm$(EXE)

modulo2:
	$(CXX) $(CXXFLAGS) $(STATIC_FLAGS) \
		modulos/02_fila_template_excecoes/src/main_teste.cpp \
		-o modulos/02_fila_template_excecoes/bin/teste_fila$(EXE)

modulo3:
	$(CXX) $(CXXFLAGS) $(STATIC_FLAGS) \
		modulos/03_polimorfismo_tarifas/src/Veiculo.cpp \
		modulos/03_polimorfismo_tarifas/src/Carro.cpp \
		modulos/03_polimorfismo_tarifas/src/Moto.cpp \
		modulos/03_polimorfismo_tarifas/src/Caminhao.cpp \
		modulos/03_polimorfismo_tarifas/src/main_teste.cpp \
		-o modulos/03_polimorfismo_tarifas/bin/teste_polimorfismo$(EXE)

sistema:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(STATIC_FLAGS) $(SISTEMA_SRCS) \
		-o $(SISTEMA_DIR)/bin/smart_parking$(EXE)

test: all
	./modulos/01_fsm_cancela/bin/teste_fsm$(EXE)
	./modulos/02_fila_template_excecoes/bin/teste_fila$(EXE)
	./modulos/03_polimorfismo_tarifas/bin/teste_polimorfismo$(EXE)
	./$(SISTEMA_DIR)/bin/smart_parking$(EXE)

clean:
	rm -f modulos/01_fsm_cancela/bin/teste_fsm$(EXE) \
	      modulos/02_fila_template_excecoes/bin/teste_fila$(EXE) \
	      modulos/03_polimorfismo_tarifas/bin/teste_polimorfismo$(EXE) \
	      $(SISTEMA_DIR)/bin/smart_parking$(EXE)

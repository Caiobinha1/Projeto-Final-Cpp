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

FINAL_DIR := EntregaFinal_Sistema_Completo
FINAL_SRCS := \
	$(FINAL_DIR)/src/FSM_Controlador.cpp \
	$(FINAL_DIR)/src/Veiculo.cpp \
	$(FINAL_DIR)/src/Carro.cpp \
	$(FINAL_DIR)/src/Moto.cpp \
	$(FINAL_DIR)/src/Caminhao.cpp \
	$(FINAL_DIR)/src/RelogioCalendario.cpp \
	$(FINAL_DIR)/src/Estacionamento.cpp \
	$(FINAL_DIR)/src/main.cpp

.PHONY: all entrega1 entrega2 entrega3 final test clean

all: entrega1 entrega2 entrega3 final

entrega1:
	$(CXX) $(CXXFLAGS) $(STATIC_FLAGS) \
		Entrega1_FSM_Modelagem/src/FSM_Controlador.cpp \
		Entrega1_FSM_Modelagem/src/main_entrega1.cpp \
		-o Entrega1_FSM_Modelagem/bin/entrega1$(EXE)

entrega2:
	$(CXX) $(CXXFLAGS) $(STATIC_FLAGS) \
		Entrega2_Estruturas_Dados/src/main_entrega2.cpp \
		-o Entrega2_Estruturas_Dados/bin/entrega2$(EXE)

entrega3:
	$(CXX) $(CXXFLAGS) $(STATIC_FLAGS) \
		Entrega3_Polimorfismo_Tarifas/src/Veiculo.cpp \
		Entrega3_Polimorfismo_Tarifas/src/Carro.cpp \
		Entrega3_Polimorfismo_Tarifas/src/Moto.cpp \
		Entrega3_Polimorfismo_Tarifas/src/Caminhao.cpp \
		Entrega3_Polimorfismo_Tarifas/src/main_entrega3.cpp \
		-o Entrega3_Polimorfismo_Tarifas/bin/entrega3$(EXE)

final:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(STATIC_FLAGS) $(FINAL_SRCS) \
		-o $(FINAL_DIR)/bin/smart_parking$(EXE)

test: all
	./Entrega1_FSM_Modelagem/bin/entrega1$(EXE)
	./Entrega2_Estruturas_Dados/bin/entrega2$(EXE)
	./Entrega3_Polimorfismo_Tarifas/bin/entrega3$(EXE)
	./$(FINAL_DIR)/bin/smart_parking$(EXE)

clean:
	rm -f Entrega1_FSM_Modelagem/bin/entrega1$(EXE) \
	      Entrega2_Estruturas_Dados/bin/entrega2$(EXE) \
	      Entrega3_Polimorfismo_Tarifas/bin/entrega3$(EXE) \
	      $(FINAL_DIR)/bin/smart_parking$(EXE)

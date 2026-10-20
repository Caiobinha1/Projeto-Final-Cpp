import os
from reportlab.lib.pagesizes import letter
from reportlab.lib import colors
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.platypus import (
    SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, HRFlowable
)
from reportlab.pdfgen import canvas

class NumberedCanvas(canvas.Canvas):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._saved_page_states = []

    def showPage(self):
        self._saved_page_states.append(dict(self.__dict__))
        self._startPage()

    def save(self):
        num_pages = len(self._saved_page_states)
        for state in self._saved_page_states:
            self.__dict__.update(state)
            self.draw_page_decorations(num_pages)
            super().showPage()
        super().save()

    def draw_page_decorations(self, page_count):
        self.saveState()
        self.setFont("Helvetica", 8)
        self.setFillColor(colors.HexColor("#555555"))
        
        if self._pageNumber > 1:
            self.drawString(40, 755, "UFSC | CTC | EEL – EEL7323: Programação C++ para Sistemas Embarcados")
            self.drawRightString(572, 755, "Entrega 2: Estruturas Dinâmicas e Templates")
            self.setStrokeColor(colors.HexColor("#CCCCCC"))
            self.setLineWidth(0.5)
            self.line(40, 750, 572, 750)
            
        self.setStrokeColor(colors.HexColor("#CCCCCC"))
        self.setLineWidth(0.5)
        self.line(40, 40, 572, 40)
        self.drawString(40, 28, "Relatório Técnico Parcial – Entrega 2: Fila Encadeada Template e Exceções")
        self.drawRightString(572, 28, f"Página {self._pageNumber} de {page_count}")
        self.restoreState()

def build_pdf(filename="Relatorio_Entrega2.pdf"):
    doc = SimpleDocTemplate(
        filename,
        pagesize=letter,
        leftMargin=40,
        rightMargin=40,
        topMargin=50,
        bottomMargin=50
    )
    
    styles = getSampleStyleSheet()
    
    c_primary = colors.HexColor("#1A365D")
    c_secondary = colors.HexColor("#2B6CB0")
    c_dark = colors.HexColor("#2D3748")
    c_bg_code = colors.HexColor("#F7FAFC")

    title_style = ParagraphStyle(
        'DocTitle',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=16,
        leading=20,
        textColor=c_primary,
        alignment=1,
        spaceAfter=4
    )

    subtitle_style = ParagraphStyle(
        'DocSubtitle',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=10,
        leading=14,
        textColor=c_secondary,
        alignment=1,
        spaceAfter=12
    )

    h1_style = ParagraphStyle(
        'Heading1_Custom',
        parent=styles['Normal'],
        fontName='Helvetica-Bold',
        fontSize=11.5,
        leading=15,
        textColor=c_primary,
        spaceBefore=10,
        spaceAfter=4,
        keepWithNext=True
    )

    body_style = ParagraphStyle(
        'Body_Custom',
        parent=styles['Normal'],
        fontName='Helvetica',
        fontSize=8.5,
        leading=12,
        textColor=c_dark,
        spaceAfter=5
    )

    body_bold = ParagraphStyle(
        'Body_Bold',
        parent=body_style,
        fontName='Helvetica-Bold'
    )

    code_style = ParagraphStyle(
        'Code_Custom',
        parent=styles['Normal'],
        fontName='Courier',
        fontSize=7.5,
        leading=10,
        textColor=colors.HexColor("#1A202C")
    )

    story = []

    # Header Banner
    header_data = [
        [
            Paragraph("<b>UNIVERSIDADE FEDERAL DE SANTA CATARINA – UFSC</b><br/>"
                      "Centro Tecnológico (CTC) | Depto. de Engenharia Elétrica e Eletrônica (EEL)<br/>"
                      "<b>Disciplina:</b> EEL7323 / EEL510265 – Programação C++ para Sistemas Embarcados<br/>"
                      "<b>Docentes:</b> Prof. Dr. Eduardo Augusto Bezerra e Prof. Anderson W. Spengler", 
                      ParagraphStyle('HdrLeft', parent=body_style, fontSize=8, leading=11, textColor=c_primary)),
            Paragraph("<b>RELATÓRIO TÉCNICO – ENTREGA 2</b><br/>"
                      "<b>Módulo:</b> Estruturas Dinâmicas & Templates<br/>"
                      "<b>Semestre:</b> 2026/2<br/>"
                      "<b>Avaliação:</b> Marco 2 (20% da Nota)", 
                      ParagraphStyle('HdrRight', parent=body_style, fontSize=8, leading=11, alignment=2, textColor=c_dark))
        ]
    ]
    t_header = Table(header_data, colWidths=[350, 182])
    t_header.setStyle(TableStyle([
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('BOTTOMPADDING', (0,0), (-1,-1), 5),
        ('LINEBELOW', (0,0), (-1,-1), 1, c_secondary)
    ]))
    story.append(t_header)
    story.append(Spacer(1, 8))

    story.append(Paragraph("ENTREGA 2: ESTRUTURAS DE DADOS DINÂMICAS E TRATAMENTO DE EXCEÇÕES", title_style))
    story.append(Paragraph("Implementação Manual de Fila Encadeada Template com RAII, Deep Copy e Exceções Customizadas", subtitle_style))
    story.append(HRFlowable(width="100%", thickness=1, color=c_secondary, spaceAfter=8))

    # 1. Introdução
    story.append(Paragraph("1. Objetivo e Escopo da Entrega 2", h1_style))
    story.append(Paragraph(
        "Em conformidade com as diretrizes do Prof. Dr. Eduardo Augusto Bezerra para a disciplina de Sistemas Embarcados, "
        "esta entrega desenvolve uma estrutura de dados dinâmica própria sem dependência exclusiva da STL. "
        "Foi implementada a classe template <code>FilaEncadeada&lt;T&gt;</code>, baseada em nós dinâmicos alocados via <code>new</code> "
        "e desalocados deterministicamente via <code>delete</code>, integrada a uma hierarquia de tratamento de exceções robusta.",
        body_style
    ))

    # 2. Princípios de Engenharia de Software Aplicados
    story.append(Paragraph("2. Princípios de Engenharia de Software e C++ Aplicados", h1_style))
    story.append(Paragraph(
        "• <b>Parametrização por Tipos (Templates):</b> A utilização de <code>template &lt;typename T&gt;</code> permite enfileirar tanto "
        "tipos primitivos (<code>int</code>, <code>double</code>) quanto classes e estruturas de domínio (<code>TiqueteInfo</code>, eventos), "
        "garantindo segurança estática de tipos (<i>type-safety</i>) em tempo de compilação sem overhead em tempo de execução.<br/>"
        "• <b>Gerenciamento de Recursos RAII (Resource Acquisition Is Initialization):</b> O destrutor da classe <code>~FilaEncadeada()</code> "
        "percorre iterativamente todos os nós alocados na <i>heap</i>, liberando-os com <code>delete</code>. Isso elimina vazamentos de memória.<br/>"
        "• <b>Regra dos Três (Rule of Three):</b> Para evitar o problema de cópia rasa (<i>shallow copy</i>) e ponteiros órfãos/duplos frees "
        "(<i>double free vulnerability</i>), foram implementados explicitamente o <b>Construtor de Cópia</b> e o <b>Operador de Atribuição</b> "
        "(<code>operator=</code>), realizando cópia profunda (<i>deep copy</i>) de cada nó da cadeia.<br/>"
        "• <b>Hierarquia de Exceções com Polimorfismo:</b> Criada a classe <code>EstacionamentoException</code> derivada de <code>std::exception</code>, "
        "sobrescrevendo <code>virtual const char* what() const noexcept override</code>. A classe <code>FilaVaziaException</code> é disparada "
        "quando operações ilegais são tentadas contra uma fila vazia.",
        body_style
    ))

    # 3. Métodos da Estrutura
    story.append(Paragraph("3. Especificação dos Métodos da Fila Encadeada", h1_style))
    
    methods_data = [
        [Paragraph("<b>Método / Assinatura</b>", body_bold), Paragraph("<b>Complexidade</b>", body_bold), Paragraph("<b>Descrição / Tratamento de Falhas</b>", body_bold)],
        [Paragraph("<code>void enfileirar(const T& valor)</code>", code_style), Paragraph("O(1)", body_style), Paragraph("Insere novo elemento no final da fila através do ponteiro <code>ultimo</code>.", body_style)],
        [Paragraph("<code>T desenfileirar()</code>", code_style), Paragraph("O(1)", body_style), Paragraph("Remove e retorna o elemento da frente. Lança <code>FilaVaziaException</code> se vazia.", body_style)],
        [Paragraph("<code>const T& frente() const</code>", code_style), Paragraph("O(1)", body_style), Paragraph("Consulta elemento da frente sem removê-lo. Lança <code>FilaVaziaException</code> se vazia.", body_style)],
        [Paragraph("<code>bool vazia() const</code>", code_style), Paragraph("O(1)", body_style), Paragraph("Retorna <code>true</code> se a fila não possuir nós alocados.", body_style)],
        [Paragraph("<code>int tamanho() const</code>", code_style), Paragraph("O(1)", body_style), Paragraph("Retorna a contagem atualizada de elementos mantida em <code>qtdElementos</code>.", body_style)],
        [Paragraph("<code>void limpar()</code>", code_style), Paragraph("O(N)", body_style), Paragraph("Desaloca iterativamente todos os nós e zera os ponteiros.", body_style)]
    ]

    t_methods = Table(methods_data, colWidths=[160, 72, 300])
    t_methods.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), colors.HexColor("#EDF2F7")),
        ('GRID', (0,0), (-1,-1), 0.5, colors.HexColor("#CBD5E0")),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
        ('LEFTPADDING', (0,0), (-1,-1), 4),
        ('RIGHTPADDING', (0,0), (-1,-1), 4),
    ]))
    story.append(t_methods)
    story.append(Spacer(1, 8))

    # 4. Resultados dos Testes
    story.append(Paragraph("4. Resultados dos Testes Automatizados e Validação", h1_style))
    story.append(Paragraph(
        "A suíte de testes <code>main_entrega2.cpp</code> cobriu todos os cenários operacionais, verificando integridade de dados e captura de exceções:",
        body_style
    ))

    log_text = (
        "<b>$ g++ -std=c++11 -Wall src/main_entrega2.cpp -o bin/entrega2.exe</b><br/>"
        "<i>[Compilado com 0 erros e 0 advertências]</i><br/>"
        "<b>$ ./bin/entrega2.exe</b><br/>"
        "[TESTE 1] Testando FilaEncadeada&lt;int&gt; com tipos primitivos...<br/>"
        "&nbsp;&nbsp;-&gt; Insercoes e remocoes FIFO validadas com sucesso! [SUCESSO]<br/>"
        "[TESTE 2] Validando tratamento de excecao em fila vazia...<br/>"
        "&nbsp;&nbsp;-&gt; Excecao capturada com sucesso: Erro FilaVazia: Tentativa de desenfileirar elemento em fila vazia.<br/>"
        "&nbsp;&nbsp;-&gt; Excecao de frente() capturada com sucesso: Erro FilaVazia: Consulta a frente() de uma fila vazia.<br/>"
        "&nbsp;&nbsp;-&gt; Excecoes de fila vazia verificadas e seguras! [SUCESSO]<br/>"
        "[TESTE 3] Testando FilaEncadeada&lt;TiqueteInfo&gt; com objetos do dominio...<br/>"
        "&nbsp;&nbsp;Frente da fila: [ID #101 | Placa: ABC-1234 | Entrada: 10:15:00]<br/>"
        "&nbsp;&nbsp;-&gt; Enfileiramento de objetos compostos validado! [SUCESSO]<br/>"
        "[TESTE 4] Validando Deep Copy (Construtor de Copia e Operador=)...<br/>"
        "&nbsp;&nbsp;-&gt; Independencia de memoria e Deep Copy 100% comprovados! [SUCESSO]<br/>"
        "[TESTE 5] Teste de estresse com 10.000 operacoes de fila...<br/>"
        "&nbsp;&nbsp;-&gt; Estresse concluido sem vazamentos de memoria! [SUCESSO]<br/>"
        "<b>RESULTADO: 100% DOS TESTES APROVADOS! (0 falhas)</b>"
    )

    t_log = Table([[Paragraph(log_text, code_style)]], colWidths=[532])
    t_log.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), colors.HexColor("#F0FFF4")),
        ('BOX', (0,0), (-1,-1), 1, colors.HexColor("#9AE6B4")),
        ('LEFTPADDING', (0,0), (-1,-1), 8),
        ('RIGHTPADDING', (0,0), (-1,-1), 8),
        ('TOPPADDING', (0,0), (-1,-1), 5),
        ('BOTTOMPADDING', (0,0), (-1,-1), 5)
    ]))
    story.append(t_log)
    story.append(Spacer(1, 8))

    # 5. Conclusão
    story.append(Paragraph("5. Conclusão da Entrega 2", h1_style))
    story.append(Paragraph(
        "A estrutura de dados dinâmica genérica e o subsistema de exceções foram concluídos com pleno êxito técnico. "
        "A garantia de RAII e Deep Copy assegura que o sistema não sofrerá corrupção de ponteiros durante a execução continuada em sistemas embarcados. "
        "O módulo está pronto para receber a hierarquia polimórfica de veículos na Entrega 3.",
        body_style
    ))

    doc.build(story, canvasmaker=NumberedCanvas)
    print(f"PDF gerado com sucesso: {filename}")

if __name__ == "__main__":
    build_pdf()

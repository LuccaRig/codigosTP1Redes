import matplotlib.pyplot as plt
import re

# Dados fornecidos
dados = """
At time +2s client sent 1024 bytes to 10.1.3.2 port 9
At time +2.01149s server received 1024 bytes from 10.1.1.1 port 49153
At time +2.01149s server sent 1024 bytes to 10.1.1.1 port 49153
At time +2.02398s client received 1024 bytes from 10.1.3.2 port 9
At time +3s client sent 1024 bytes to 10.1.3.2 port 9
At time +3.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +3.00746s server sent 1024 bytes to 10.1.1.1 port 49153
At time +3.01493s client received 1024 bytes from 10.1.3.2 port 9
At time +4s client sent 1024 bytes to 10.1.3.2 port 9
At time +4.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +4.00746s server sent 1024 bytes to 10.1.1.1 port 49153
At time +4.01493s client received 1024 bytes from 10.1.3.2 port 9
At time +5s client sent 1024 bytes to 10.1.3.2 port 9
At time +5.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +5.00746s server sent 1024 bytes to 10.1.1.1 port 49153
At time +5.01493s client received 1024 bytes from 10.1.3.2 port 9
At time +6s client sent 1024 bytes to 10.1.3.2 port 9
At time +6.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +6.00746s server sent 1024 bytes to 10.1.1.1 port 49153
At time +6.01493s client received 1024 bytes from 10.1.3.2 port 9
At time +7s client sent 1024 bytes to 10.1.3.2 port 9
At time +7.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +7.00746s server sent 1024 bytes to 10.1.1.1 port 49153
At time +7.01493s client received 1024 bytes from 10.1.3.2 port 9
At time +8s client sent 1024 bytes to 10.1.3.2 port 9
At time +8.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +8.00746s server sent 1024 bytes to 10.1.1.1 port 49153
At time +8.01493s client received 1024 bytes from 10.1.3.2 port 9
At time +9s client sent 1024 bytes to 10.1.3.2 port 9
At time +9.00746s server received 1024 bytes from 10.1.1.1 port 49153
At time +9.00746s server sent 1024 bytes to 10.1.1.1 port 49153
At time +9.01493s client received 1024 bytes from 10.1.3.2 port 9
At time +10s client sent 1024 bytes to 10.1.3.2 port 9
At time +10.0075s server received 1024 bytes from 10.1.1.1 port 49153
At time +10.0075s server sent 1024 bytes to 10.1.1.1 port 49153
At time +10.0149s client received 1024 bytes from 10.1.3.2 port 9
At time +11s client sent 1024 bytes to 10.1.3.2 port 9
At time +11.0075s server received 1024 bytes from 10.1.1.1 port 49153
At time +11.0075s server sent 1024 bytes to 10.1.1.1 port 49153
At time +11.0149s client received 1024 bytes from 10.1.3.2 port 9
"""

# Processar os dados
linhas = dados.strip().split('\n')
eventos = []

for linha in linhas:
    # Extrair tempo e tipo de evento
    tempo_match = re.search(r'time \+([\d.]+)s', linha)
    tempo = float(tempo_match.group(1)) if tempo_match else None
    
    if tempo is not None:
        if 'client sent' in linha:
            eventos.append({'tempo': tempo, 'tipo': 'client_sent', 'descricao': 'Cliente enviou'})
        elif 'server received' in linha:
            eventos.append({'tempo': tempo, 'tipo': 'server_received', 'descricao': 'Servidor recebeu'})
        elif 'server sent' in linha:
            eventos.append({'tempo': tempo, 'tipo': 'server_sent', 'descricao': 'Servidor enviou'})
        elif 'client received' in linha:
            eventos.append({'tempo': tempo, 'tipo': 'client_received', 'descricao': 'Cliente recebeu'})

# Organizar dados por requisição
requisicoes = []
i = 0
while i < len(eventos):
    if eventos[i]['tipo'] == 'client_sent':
        req = {
            'client_sent': eventos[i]['tempo'],
            'server_received': eventos[i+1]['tempo'],
            'server_sent': eventos[i+2]['tempo'],
            'client_received': eventos[i+3]['tempo']
        }
        requisicoes.append(req)
        i += 4
    else:
        i += 1

# Calcular tempos de resposta
tempos_requisicao = [req['client_sent'] for req in requisicoes]
tempos_ida = [req['server_received'] - req['client_sent'] for req in requisicoes]
tempos_processamento = [req['server_sent'] - req['server_received'] for req in requisicoes]
tempos_volta = [req['client_received'] - req['server_sent'] for req in requisicoes]
tempos_total = [req['client_received'] - req['client_sent'] for req in requisicoes]

# Configurar estilo global para negrito e fonte maior
plt.rcParams.update({
    'font.weight': 'bold',
    'axes.labelweight': 'bold',
    'axes.titleweight': 'bold',
    'figure.titleweight': 'bold',
    'font.size': 14
})

# Criar gráfico
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(14, 10))

# Gráfico 1: Tempos de resposta por componente
bar_width = 0.6
indices = range(len(requisicoes))

bars1 = ax1.bar(indices, tempos_ida, bar_width, label='Cliente→Servidor', color='blue', alpha=0.7)
bars2 = ax1.bar(indices, tempos_processamento, bar_width, bottom=tempos_ida, label='Processamento Servidor', color='green', alpha=0.7)
bars3 = ax1.bar(indices, tempos_volta, bar_width, bottom=[i+p for i,p in zip(tempos_ida, tempos_processamento)], 
                label='Servidor→Cliente', color='red', alpha=0.7)

ax1.set_xlabel('Número da Requisição', fontweight='bold', fontsize=16)
ax1.set_ylabel('Tempo (segundos)', fontweight='bold', fontsize=16)
ax1.set_title('Tempo de Resposta por Componente Topologia com CSMA', fontweight='bold', fontsize=18)
ax1.set_xticks(indices)
ax1.legend(prop={'weight': 'bold', 'size': 12})
ax1.grid(True, alpha=0.3)

# Adicionar valores nas barras em negrito
for i, (v1, v2, v3, total) in enumerate(zip(tempos_ida, tempos_processamento, tempos_volta, tempos_total)):
    ax1.text(i, total + 0.0005, f'{total*1000:.2f}ms', ha='center', va='bottom', fontsize=10, fontweight='bold')

# Gráfico 2: Tempo total de resposta
ax2.plot(indices, [t * 1000 for t in tempos_total], 'o-', linewidth=2, markersize=8, color='purple', label='Tempo Total')
ax2.set_xlabel('Número da Requisição', fontweight='bold', fontsize=16)
ax2.set_ylabel('Tempo Total (milissegundos)', fontweight='bold', fontsize=16)
ax2.set_title('Tempo Total de Resposta por Requisição', fontweight='bold', fontsize=18)
ax2.set_xticks(indices)
ax2.grid(True, alpha=0.3)
ax2.legend(prop={'weight': 'bold', 'size': 12})

# Adicionar valores nos pontos em negrito
for i, tempo in enumerate(tempos_total):
    ax2.text(i, tempo * 1000 + 0.1, f'{tempo*1000:.2f}ms', ha='center', va='bottom', fontsize=10, fontweight='bold')

plt.tight_layout()
plt.savefig('grafico2.pdf', format='pdf', bbox_inches='tight')
plt.show()

# Estatísticas
print("\n--- Estatísticas dos Tempos de Resposta ---")

print(f"Número total de requisições: {len(requisicoes)}")
print(f"Tempo médio total: {sum(tempos_total)/len(tempos_total)*1000:.4f} ms")
print(f"Tempo médio cliente→servidor: {sum(tempos_ida)/len(tempos_ida)*1000:.4f} ms")
print(f"Tempo médio processamento: {sum(tempos_processamento)/len(tempos_processamento)*1000:.4f} ms")
print(f"Tempo médio servidor→cliente: {sum(tempos_volta)/len(tempos_volta)*1000:.4f} ms")
print(f"Tempo mínimo total: {min(tempos_total)*1000:.4f} ms")
print(f"Tempo máximo total: {max(tempos_total)*1000:.4f} ms")

# Análise comparativa
print("\n--- Análise Comparativa ---")
print("Primeira requisição vs Demais:")
print(f"  Primeira: {tempos_total[0]*1000:.4f} ms")
print(f"  Demais: {sum(tempos_total[1:])/len(tempos_total[1:])*1000:.4f} ms em média")
print(f"  Diferença: {tempos_total[0]*1000 - sum(tempos_total[1:])/len(tempos_total[1:])*1000:.4f} ms")

variancia = sum((x - sum(tempos_total)/len(tempos_total))**2 for x in tempos_total) / len(tempos_total)
print(f"Variação (desvio padrão): {(variancia**0.5)*1000:.4f} ms")
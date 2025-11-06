import matplotlib.pyplot as plt
import pandas as pd

# Lê os dados
df = pd.read_csv('goodput_results.txt', delim_whitespace=True)

# Configura o gráfico
plt.figure(figsize=(12, 7))

# Plota para cada combinação de protocolo e nflow
for protocolo in df['Tcp_Protocol'].unique():
    for nflow in sorted(df['nflows'].unique()):
        dados = df[(df['Tcp_Protocol'] == protocolo) & (df['nflows'] == nflow)]
        dados = dados.sort_values('error')
        
        plt.plot(dados['error'], dados['goodput'], 
                marker='o', linewidth=2, markersize=6,
                label=f'{protocolo} - {nflow} flow(s)')

plt.xlabel('error rate', fontsize=12)
plt.ylabel('Goodput (bps)', fontsize=12)
plt.title('Goodput vs error por Protocolo TCP e Número de Flows', fontsize=14)
plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
plt.grid(True, alpha=0.3)
plt.ticklabel_format(style='sci', axis='y', scilimits=(0,0))
plt.tight_layout()
plt.savefig('goodput_simple.png', dpi=300, bbox_inches='tight')
plt.show()  
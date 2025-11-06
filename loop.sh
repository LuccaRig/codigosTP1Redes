#!/bin/bash


DATA_RATE="10Mbps"
ERROR_RATE="0.00001"
DELAY_RATE="1ms"
NS3_SCRIPT="lab2-part1.cc"
OUTPUT_DIR="resultados"
GOODPUT_FILE="goodput_results.txt"

mkdir -p $OUTPUT_DIR
echo "delay nflows goodput" > $GOODPUT_FILE

# Valores a variar
NFLOWS=(1 2 4)
DELAYS=("50ms" "100ms" "150ms" "200ms" "250ms" "300ms")
ERROR=(0.00001 0.001)

echo "Iniciando execuções..."

# for nflow in "${NFLOWS[@]}"; do
#     for delay in "${DELAYS[@]}"; do
#         echo "Executando: nFlows=$nflow, delay=$delay"
        
#         output=$(./ns3 run $NS3_SCRIPT -- --dataRate=$DATA_RATE --delay=$delay --errorRate=$ERROR_RATE --nFlows=$nflow 2>&1)
        
#         goodput=$(echo "$output" | grep "Total Aggregate Goodput (bps):" | awk '{print $5}')
        
#         delay_number=$(echo "$delay" | sed 's/ms//')
        
#         echo "$delay_number $nflow $goodput" >> $GOODPUT_FILE
        
#         [ -f "NewReno.data" ] && mv "NewReno.data" "${OUTPUT_DIR}/NewReno_n${nflow}_d${delay}.data"
#         [ -f "Cubic.data" ] && mv "Cubic.data" "${OUTPUT_DIR}/Cubic_n${nflow}_d${delay}.data"
        
#         sleep 1
#     done
# done

for nflow in "${NFLOWS[@]}"; do
    for error in "${ERROR[@]}"; do
        echo "Executando: nFlows=$nflow, error=$error"
        
        output=$(./ns3 run $NS3_SCRIPT -- --dataRate=$DATA_RATE --delay=$delay --errorRate=$error --nFlows=$DELAY_RATE 2>&1)
        
        goodput=$(echo "$output" | grep "Total Aggregate Goodput (bps):" | awk '{print $5}')
        
        error_value=$(echo "$error" | sed 's/ms//')
        
        echo "$error_value $nflow $goodput" >> $GOODPUT_FILE
        
        [ -f "NewReno.data" ] && mv "NewReno.data" "${OUTPUT_DIR}/NewReno_n${nflow}_d${error}.data"
        [ -f "Cubic.data" ] && mv "Cubic.data" "${OUTPUT_DIR}/Cubic_n${nflow}_d${error}.data"
        
        sleep 1
    done
done

echo "Concluído!"
echo "Resultados salvos em: $GOODPUT_FILE"
echo ""
echo "Conteúdo do arquivo:"
cat $GOODPUT_FILE
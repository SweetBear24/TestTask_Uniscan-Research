#include <iostream>
#include <deque>
#include <vector>  // Используем std::vector вместо сырых указателей
#include <cstring> // Для memcpy
using namespace std;

struct Packet {
    unsigned int packetId;    // Идентификатор пакета
    vector<unsigned char> data; 
    unsigned int dataCount;   // Размер данных пакета

    Packet(unsigned int id, unsigned char* d, unsigned int count)
        : packetId(id), dataCount(count), data(d, d + count) { }

};

template<typename T>
class QueueOfPackets {
private:
    unsigned int MaxLengthOfPacket;       // Максимальная длина пакета
    unsigned int MaxAmountOfPackets;      // Максимальное количество пакетов
    unsigned int MaximumTotalSizeOfPacket;// Максимальный общий размер данных
    unsigned int CurrentTotalSize = 0;    // Текущий суммарный размер данных
    deque<T> QueueOfPacket;               // Очередь пакетов

public:
    QueueOfPackets(unsigned int maxLength, unsigned int maxAmount, unsigned int maxTotalSize)
        : MaxLengthOfPacket(maxLength), 
          MaxAmountOfPackets(maxAmount), 
          MaximumTotalSizeOfPacket(maxTotalSize) {}

    bool Push(unsigned char* data, unsigned int dataCount, unsigned int packetId) {
        if (dataCount <= MaxLengthOfPacket &&
            QueueOfPacket.size() < MaxAmountOfPackets &&
            CurrentTotalSize + dataCount <= MaximumTotalSizeOfPacket) {
            
            // Создаем новый пакет и добавляем его в очередь
            T newPacket(packetId, data, dataCount); 
            QueueOfPacket.push_back(newPacket);
            CurrentTotalSize += dataCount; 
            return true; // Пакет успешно добавлен
        }
        return false; // Не удалось добавить пакет
    }

    bool Pop(unsigned char* buffer, unsigned int bufferSize, unsigned int& dataCount, unsigned int& packetId) {
        if (QueueOfPacket.empty()) {
            return false; // Очередь пуста, пакет не извлечен
        }

        // Извлекаем первый пакет
        T& frontPacket = QueueOfPacket.front(); 

        dataCount = frontPacket.dataCount; // Получаем фактический размер пакета
        packetId = frontPacket.packetId;   // Получаем идентификатор пакета

        if (bufferSize < dataCount) {
            memcpy(buffer, frontPacket.data.data(), bufferSize); // Копируем часть данных
        } else {
            memcpy(buffer, frontPacket.data.data(), dataCount); // Копируем весь пакет
        }

        // Удаляем пакет из очереди
        QueueOfPacket.pop_front();
        CurrentTotalSize -= frontPacket.dataCount; // Обновляем текущий суммарный размер данных

        return true; // Пакет успешно извлечен
    }
};

int main() {
    QueueOfPackets<Packet> queue(1024, 10, 4096); // Создаем очередь с ограничениями
    unsigned char data[] = { 0x01, 0x02, 0x03, 0x04 };
    unsigned int dataCount = sizeof(data);

    if (queue.Push(data, dataCount, 1)) {
        cout << "Packet added." << endl;
    } else {
        cout << "Failed to add packet." << endl;
    }

    unsigned char buffer[1024];
    unsigned int extractedDataCount;
    unsigned int extractedPacketId;

    if (queue.Pop(buffer, sizeof(buffer), extractedDataCount, extractedPacketId)) {
        cout << "Packet extracted. ID: " << extractedPacketId 
             << ", Data size: " << extractedDataCount << endl;
    } else {
        cout << "Queue is empty!" << endl;
    }

    return 0;
}

#ifndef PACKETDECODER_H
#define PACKETDECODER_H

#include <QObject>
#include "packet.h"
#include "datatype.h"
#include "formattype.h"
#include "memorylocationinfo.h"
#include <QMap>

class PacketDecoder : public QObject
{
	Q_OBJECT
public:
	explicit PacketDecoder(QObject *parent = 0);
	Packet parseBuffer(QByteArray buffer);
public slots:
	void parsePacket(Packet parsedPacket);
	void parseBufferToPacket(QByteArray buffer);
private:
	QList<LocationIdFlags> m_blockFlagList;
	QMap<LocationIdFlags,QString> m_blockFlagToNameMap;
	QByteArray m_partialPacketBuffer;
	unsigned char m_currentPartialPacketSequence;
	bool isPartialPacketWaiting;
signals:
	void decoderName(QString name);
	void firmwareBuild(QString version);
	void compilerVersion(QString version);
	void operatingSystem(QString version);
	void dataLogPayloadReceived(QByteArray header,QByteArray payload);
	void interfaceVersion(QString version);
	void firmwareVersion(QString version);
	void builtByName(QString name);
	void supportEmail(QString email);
	void unknownPacket(QByteArray header,QByteArray payload);
	void commandSuccessful(int sequencenum);
	void commandFailed(int sequencenum,unsigned short errornum);
	void datalogDescriptor(QString data);
	void fieldDescriptor(QString data);
	void tableDescriptor(QString data);
	void packetNaked(unsigned short payloadid,QByteArray header,QByteArray payload,unsigned short errornum);
	void packetAcked(unsigned short payloadid,QByteArray header,QByteArray payload);
	void partialPacketAcked(unsigned short payloadid,QByteArray header,QByteArray payload);
	void completePacketAcked(unsigned short payloadid,QByteArray header,QByteArray payload);
	void locationIdList(QList<unsigned short> locationidlist);
	void locationIdInfo(MemoryLocationInfo info);
	void flashBlockUpdatePacket(QByteArray header,QByteArray payload);
	void ramBlockUpdatePacket(QByteArray header,QByteArray payload);
	void benchTestReply(unsigned short countRemaining,unsigned char currentEvent);
	void firmwareDebug(QString text);
};

#endif // PACKETDECODER_H

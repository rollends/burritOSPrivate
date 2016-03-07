#ifndef TRAIN_COMMANDER_H
#define TRAIN_COMMANDER_H

S32 pushTrainCommand(String string);
S32 pollTrainCommand(U8 trainId, MessageEnvelope* env);
void TrainCommanderServer(void);

#endif

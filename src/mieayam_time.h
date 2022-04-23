#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void MieAyam_TimeInit(void);
void MieAyam_TimeCount(void);

float MieAyam_GetTimeInMicroseconds(void);
float MieAyam_GetTimeInMiliseconds(void);
float MieAyam_GetTimeInSeconds(void);

#ifdef __cplusplus
}
#endif
C     
      SUBROUTINE SKGETV
C
      IMPLICIT NONE

#include "vcwork.h"      
#include "mcvector_commons.h"

      INTEGER I

C
C     CLEAR COMMON
C
      POSA(1)=0.
      POSA(2)=0.
      POSA(3)=0.
      DO 10 I=1,50
         IPA(I)=0
         PIN(1,I)=0.
         PIN(2,I)=0.
         PIN(3,I)=0.
         PABS(I)=0.
   10 CONTINUE
      NVECT=0
C
C     READ VECTOR
C      
      CALL VCRDVCCM    ! mcvect and mcvertex bank
C
C    PUT COMMON
C
      POSA(1)=POSIVC(1,1)
      POSA(2)=POSIVC(2,1)
      POSA(3)=POSIVC(3,1)
      DO 500 I=1,NVC
         IF (I.GE.5 .AND. IFLGVC(I).NE.0) GOTO 500
         NVECT=NVECT+1
         CALL PD2GT(IPVC(I),IPA(NVECT))
         PIN(1,NVECT)=PVC(1,I)
         PIN(2,NVECT)=PVC(2,I)
         PIN(3,NVECT)=PVC(3,I)
         PABS(NVECT)=SQRT(PIN(1,NVECT)**2+PIN(2,NVECT)**2
     $        +PIN(3,NVECT)**2)
c MF
         CHERENKOV(NVECT) = ICRNVC(I)
         IF(NVECT.GE.MAXVECT) GOTO 501
 500  CONTINUE
 501  CONTINUE
c      


      RETURN
      END

--2.3(A)
SELECT (FIRST_NAME||' '||LAST_NAME) NAME, SALARY , NVL(COMMISSION_PCT,0)
FROM EMPLOYEES
WHERE JOB_ID ='AD_PRES'
ORDER BY COMMISSION_PCT  ASC, SALARY  DESC;

--2.3(B)
SELECT COUNTRY_NAME 
FROM COUNTRIES 
ORDER BY COUNTRY_NAME  ASC;
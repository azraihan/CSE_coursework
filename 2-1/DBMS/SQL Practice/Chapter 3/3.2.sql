--3.2(A)
SELECT LAST_NAME , ROUND(SYSDATE-HIRE_DATE,2) DAYS_EMPLOYED
FROM EMPLOYEES;

--3.2(B)
SELECT LAST_NAME , TRUNC((SYSDATE-HIRE_DATE)/365,3) DAYS_EMPLOYED
FROM EMPLOYEES;
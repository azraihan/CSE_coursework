--4.1(A)
SELECT MANAGER_ID, COUNT(*) EMPLOYEES_MANAGED
FROM EMPLOYEES
GROUP BY MANAGER_ID ;

--4.1(B)
SELECT DEPARTMENT_ID, COUNT(SALARY) NUMNER_OF_EMPLOYEES
FROM EMPLOYEES
WHERE SALARY > 10000
GROUP BY DEPARTMENT_ID;

--4.1(C)
SELECT DEPARTMENT_ID, MIN(SALARY) MIN, MAX(SALARY) MAX, AVG(SALARY) AVG 
FROM EMPLOYEES
WHERE  DEPARTMENT_ID <> 80
GROUP BY DEPARTMENT_ID
ORDER BY AVG DESC, MAX DESC, MIN DESC;
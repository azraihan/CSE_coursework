--2.1(A)

SELECT COUNTRY_NAME
FROM COUNTRIES;

--2.1(B)
SELECT JOB_TITLE
FROM JOBS;

--2.1(C)
SELECT MANAGER_ID
FROM EMPLOYEES e ;

--2.1(D)
SELECT CITY
FROM LOCATIONS;

--2.1(E)
SELECT LOCATION_ID, (STREET_ADDRESS||', '||CITY||', '||STATE_PROVINCE||', '||POSTAL_CODE) ADDRESS
FROM LOCATIONS;
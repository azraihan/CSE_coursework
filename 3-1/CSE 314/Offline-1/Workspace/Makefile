SCRIPT = organize.sh
PATH_OF_SUBMISSION_FOLDER = submissions
PATH_OF_TARGET_FOLDER = targets
PATH_OF_TEST_FOLDER = tests
PATH_OF_ANSWER_FOLDER = answers


run: 
	./$(SCRIPT) $(PATH_OF_SUBMISSION_FOLDER) $(PATH_OF_TARGET_FOLDER) $(PATH_OF_TEST_FOLDER) $(PATH_OF_ANSWER_FOLDER)

clean:
	rm -r $(PATH_OF_TARGET_FOLDER)

permission:
	chmod +x $(SCRIPT)

no_permission:
	chmod -x $(SCRIPT)

.PHONY: run permission no_permission

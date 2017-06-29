(global-set-key "\C-cpn" 'flp-pse-new-appointment)

(defun flp-pse-get-next-id (prefix)
 (interactive)
 (concat prefix (read-from-minibuffer "Next ID: ")))

(defun flp-pse-render (term)
 (interactive)
 (if (= (length term) 1)
  (car term)
  (concat "groupFn(" (join "," term) "))")))

(defun flp-pse-goto-end ()
 ""
 (end-of-buffer)
 (delete-blank-lines)
 (delete-blank-lines)
 (open-line 1)
 (end-of-buffer))

(defun flp-pse-new-appointment ()
 ""
 (interactive)
 (flp-load-tasks)
 (flp-pse-goto-end)
 (let* ((appointmentid (flp-pse-get-next-id "appointment"))
	(our-participants (list "andrewDougherty"))
	(event-specification "meetingWith(test)"))
  (insert (concat "appointment(" appointmentid "," (flp-pse-render our-participants) "," event-specification ").\n"))
  (insert (concat "deadline(" appointmentid "," (flp-prolog-timestamp) ").\n"))))

(provide 'free-life-planner-planning-scheduling-and-execution)

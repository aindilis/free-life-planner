(global-set-key "\C-calexa" 'alexa-skill-flp-quick-start)
(global-set-key "\C-calexp" 'alexa-integration-open-cached-utterances-directory)

(defvar alexa-skill-flp-buffer-name "*Alexa-Skill-FLP*")

(defun alexa-skill-flp-quick-start (&optional proceed-anyway)
 ""
 (interactive)
 (if (and
      (kmax-buffer-exists-p alexa-skill-flp-buffer-name)
      (not proceed-anyway))
  (switch-to-buffer alexa-skill-flp-buffer-name)
  (run-in-shell
   "cd /var/lib/myfrdcsa/codebases/minor/alexa-integration/sample-custom-skill-as-web-service && ./start-alexa-skill-flp-unilang.sh "
   alexa-skill-flp-buffer-name
   nil)))

(defun alexa-integration-open-cached-utterances-directory ()
 ""
 (interactive)
 (ffap "/var/lib/myfrdcsa/codebases/minor/alexa-integration/data-git/push-notifications/cache"))

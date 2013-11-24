Running Test Plans
==

Prerequisites: you must have a github account; we're using github to store test results, and to keep track of issues.

Configure orb-min/orb-cave to log timestamps in its debug.log file (see the 'sample.alravaiorb.conf') put it
in the data directory.


What should you test?
--

Generally, testers are recruited by developers or QA leads by asking for help on forums, in IRC chat, in pull request comments, on twitter, etc.

The developer or QA lead will create the test plan in a repository here at github, usually by forking the main QA repository. So you can look at its forking history to see testing activity or find things that might need more testing.


Test procedure
--

1. Fork the github repository that has the test plan you'll be working on.<br/>
  ![github fork](http://dl.dropbox.com/u/38065353/Github_ForkButton.jpg)
  
2. Do what it says in the test plan.

3. File any issues **in the main alraviaorb repository issue tracker!** If you just poke the Issues button in your forked test plan repository the developers will never see them.

4. Edit the copy of the test plan in your repository with the results of your testing. Feel free to add notes, etc.

5. Save a copy of the debug.log file(s) created by orb-mine/orb-cave in case the developers need them to find issues or to prove that you tested what you said you tested in order to claim a testing bounty.

When you have finished running the test plan, submit a pull request or send a message to the person who created the test plan to let them know you've finished testing.<br/>
  ![github fork](http://dl.dropbox.com/u/38065353/Github_PullRequest.jpg)

  
Bounties
--

First: do not expect to get rich as a AlraviaOrb reference implementation QA tester. Your main motivation for helping test should be to move the AlraviaOrb project forward, not to make money. Think of bounties as thank-you tips for tackling a job that most people don't want to do.

If there is a orb bounty for running a test plan, then the developer or QA lead will decide how to distribute the funds. If you think they are being unfair, try to stay calm and be constructive: suggest how they might do a better job next time. This is all a big experiment; don't forget that you are part of the process and will help make it succeed or fail.

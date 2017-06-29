(use Formalog to handle this, make it like critic.  Have it
 record when it's explicitly told by the system that one is more
 important than the other.  record this using fassert.  When
 running in review mode, have it iterate over all the explicit
 ordering statements and ask us if they are still valid.  or have
 us add new ones and detect which ones are violated and ask us to
 decide on whether they should remain the same or not.  When in
 add mode (i.e. adding new ordering assertions), we will want to
 use the Formalog inference to determine if the current KB
 implies that one is of greater importance than the other.  If it
 doesn't decide it, we can ask the user.  Figure out what to do
 in the case that we explicitly assert that either we do not know
 which is more important, or that neither is more important or
 that they are incomparable, etc.)

(after we get this system, which we may want to add to critic,
 working better, we can use it to rank all of our todo items for
 the life planner and then work on adding stuff to the life
 planner (including this critic module itself) in order to
 improve it most usefully)

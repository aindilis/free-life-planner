warnUser('Appt: ') :-
  hasAppointment(andrewDougherty,Date),
  dayOfWeekOfDate(Date,tuesday),

dayOfWeekOfDate(Date,Output) :-
    format_time(Output,'u',Date).

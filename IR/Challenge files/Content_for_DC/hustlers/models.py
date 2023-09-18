from django.db import models

from django.contrib.auth.models import User

class Reservation(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    STOP_CHOICES = [
        ('Harlem', 'West Side Highway near 116th Street'),
        ('Upper West Side', 'Riverside Park near 86th Street'),
        #... add all the other stops here following the same format
        ('Greenpoint', 'Newtown Creek near Greenpoint Avenue')
    ]
    start_stop = models.CharField(max_length=100, choices=STOP_CHOICES)
    end_stop = models.CharField(max_length=100, choices=STOP_CHOICES)
    date = models.DateField()
    time = models.TimeField()

    def __str__(self):
        return f"{self.user.username}'s Reservation on {self.date} from {self.start_stop} to {self.end_stop}"


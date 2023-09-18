from django.shortcuts import render
from django.http import HttpResponse
from django.contrib.auth import authenticate, login, logout
from .models import Reservation
from django.contrib.auth.decorators import login_required

# Create your views here.

# hey if you are a ctf player and have reached this point, good job! 
# you might want to check in the place where django conventially stores static files 


# the default route 
def index(request):
    return render(request,"hustlers/index.html")

@login_required
def make_reservation(request):
    if request.method == 'POST':
        start_stop = request.POST['start_stop']
        end_stop = request.POST['end_stop']
        date = request.POST['date']
        time = request.POST['time']

        reservation = Reservation(user=request.user, start_stop=start_stop, end_stop=end_stop, date=date, time=time)
        reservation.save()

        messages.success(request, 'Reservation successfully made!')
        return redirect('index')  # or wherever you want to redirect after a successful reservation

    return render(request, 'reservations.html')

def login_view(request):
    if request.method == "POST":

        # Attempt to sign user in
        username = request.POST["username"]
        password = request.POST["password"]
        user = authenticate(request, username=username, password=password)

        # Check if authentication successful
        if user is not None:
            login(request, user)
            return HttpResponseRedirect(reverse("index"))
        else:
            return render(request, "hustlers/login.html", {
                "message": "Invalid username and/or password."
            })
    else:
        return render(request, "hustlers/login.html")


def logout_view(request):
    logout(request)
    return HttpResponseRedirect(reverse("index"))


def register(request):
    if request.method == "POST":
        username = request.POST["username"]
        email = request.POST["email"]

        # Ensure password matches confirmation
        password = request.POST["password"]
        confirmation = request.POST["confirmation"]
        if password != confirmation:
            return render(request, "hustlers/register.html", {
                "message": "Passwords must match."
            })

        # Attempt to create new user
        try:
            user = User.objects.create_user(username, email, password)
            user.save()
        except IntegrityError:
            return render(request, "hustlers/register.html", {
                "message": "Username already taken."
            })
        login(request, user)
        return HttpResponseRedirect(reverse("index"))
    else:
        return render(request, "hustlers/register.html")
    





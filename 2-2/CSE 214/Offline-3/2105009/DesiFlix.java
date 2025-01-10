import StreamingPlatform.*;
import Observer.ConcreteObserver;
import Observer.Observer;

public class DesiFlix {
    public static void main(String[] args){
        // Initialize streaming platforms for specific genres
        MovieStreamingPlatform ComedyPlatform = new MovieStreamingPlatformForSpecificGenre("Comedy");
        MovieStreamingPlatform ThrillerPlatform = new MovieStreamingPlatformForSpecificGenre("Thriller");
        MovieStreamingPlatform HorrorPlatform = new MovieStreamingPlatformForSpecificGenre("Horror");

        // Create movie instances
        Movie ComedyMovie1 = new Movie("Superbad", "Comedy");
        Movie ComedyMovie2 = new Movie("Step Brothers", "Comedy");
        Movie ThrillerMovie1 = new Movie("Se7en", "Thriller");
        Movie ThrillerMovie2 = new Movie("Gone Girl", "Thriller");
        Movie HorrorMovie1 = new Movie("A Nightmare on Elm Street", "Horror");
        Movie HorrorMovie2 = new Movie("Hereditary", "Horror");

        // Create observer instances (users)
        Observer Observer1 = new ConcreteObserver("Observer1");
        Observer Observer2 = new ConcreteObserver("Observer2");
        Observer Observer3 = new ConcreteObserver("Observer3");

        // Users subscribe to different platforms
        Observer3.subscribeTo(HorrorPlatform);
        Observer1.subscribeTo(ComedyPlatform);
        Observer2.subscribeTo(ThrillerPlatform);
        Observer2.subscribeTo(ComedyPlatform);
        Observer1.subscribeTo(ThrillerPlatform);
        Observer3.subscribeTo(ComedyPlatform);
        Observer3.subscribeTo(ThrillerPlatform);

        // Display subscribers for each platform
        ComedyPlatform.getSubscribers();
        ThrillerPlatform.getSubscribers();
        HorrorPlatform.getSubscribers();

        // Upload movies in a different sequence
        HorrorPlatform.uploadMovie(HorrorMovie2);
        ComedyPlatform.uploadMovie(ComedyMovie1);
        ThrillerPlatform.uploadMovie(ThrillerMovie1);

        ComedyPlatform.uploadMovie(ComedyMovie2);
        ThrillerPlatform.uploadMovie(ThrillerMovie2);
        HorrorPlatform.uploadMovie(HorrorMovie1);
    }
}

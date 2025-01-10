package StreamingPlatform;

import java.util.List;
import Observer.Observer;

public abstract class MovieStreamingPlatform {
    private final String genre;
    protected List<Observer> observerList;
    protected List<Movie> movieList;

    public MovieStreamingPlatform(String genre)
    {
        this.genre = genre;
    }

    public String getGenre()
    {
        return this.genre;
    }

    public abstract void registerObserver(Observer observer);

    public abstract void removeObserver(Observer observer);

    public abstract void notifyObservers(Movie movie);

    public abstract void uploadMovie(Movie movie);

    public abstract void getAllMovies();

    public abstract void getSubscribers();
}

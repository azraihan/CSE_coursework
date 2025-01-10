package StreamingPlatform;

import java.util.ArrayList;
import Observer.Observer;

public class MovieStreamingPlatformForSpecificGenre extends MovieStreamingPlatform{
    public MovieStreamingPlatformForSpecificGenre(String genre) 
    {
        super(genre);
        this.observerList = new ArrayList<>();
        this.movieList = new ArrayList<>();
    }

    @Override
    public void registerObserver(Observer observer) 
    {
        this.observerList.add(observer);
    }

    @Override
    public void removeObserver(Observer observer) 
    {
        this.observerList.remove(observer);
    }

    @Override
    public void notifyObservers(Movie movie) 
    {
        for (Observer observer : this.observerList) 
        {
            observer.getNotification(this, movie);
        }
    }

    @Override
    public void uploadMovie(Movie movie) 
    {
        this.movieList.add(movie);
        Thread thread = new Thread(() -> 
            notifyObservers(movie)
        );
        thread.start();
    }

    @Override
    public void getAllMovies() 
    {
        for (Movie movie : this.movieList) 
        {
            System.out.println(movie);
        }
    }

    @Override
    public void getSubscribers() 
    {
        System.out.println("Subscribers for " + this.getGenre() + " Platform:");
        for (Observer observer : this.observerList) 
        {
            System.out.println(observer.getObserverName());
        }
    }
}

package Observer;

import java.util.ArrayList;
import java.util.List;
import StreamingPlatform.Movie;
import StreamingPlatform.MovieStreamingPlatform;


public class ConcreteObserver implements Observer{
    private final String name;
    private List<String> genreList;

    public ConcreteObserver(String name)
    {
        this.name = name;
        this.genreList = new ArrayList<>();
    }

    @Override
    public void getNotification(MovieStreamingPlatform moviePlatform, Movie movie) 
    {
        System.out.println("Hello " + this.name + ", the movie " + movie.getTitle() + " has been added to " + moviePlatform.getGenre() + " genre.");
    }

    @Override
    public String getObserverName() 
    {
        return this.name;
    }

    @Override
    public void subscribeTo(MovieStreamingPlatform moviePlatform) 
    {
        moviePlatform.registerObserver(this);
        genreList.add(moviePlatform.getGenre());
    }

    @Override
    public void unsubscribeFrom(MovieStreamingPlatform moviePlatform) 
    {
        moviePlatform.removeObserver(this);
        genreList.remove(moviePlatform.getGenre());
    }

    @Override
    public void getFavGenreDetails() 
    {
        System.out.println("Hello " + this.name + ", the genres you are subscribed to are: ");
        for (String genre : genreList)
        {
            System.out.println(genre);
        }
    }
}

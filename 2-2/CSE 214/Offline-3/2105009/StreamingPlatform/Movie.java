package StreamingPlatform;

public class Movie 
{
    private final String title;
    private final String genre;

    public Movie(String title, String genre) 
    {
        this.title = title;
        this.genre = genre;
    }

    public String getTitle() 
    {
        return this.title;
    }

    public String getGenre() 
    {
        return this.genre;
    }
}

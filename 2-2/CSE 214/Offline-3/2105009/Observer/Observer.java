package Observer;

import StreamingPlatform.Movie;
import StreamingPlatform.MovieStreamingPlatform;

public interface Observer 
{
    void getNotification(MovieStreamingPlatform moviePlatform, Movie movie);
    String getObserverName();
    void subscribeTo(MovieStreamingPlatform moviePlatform);
    void unsubscribeFrom(MovieStreamingPlatform moviePlatform);
    void getFavGenreDetails();
}

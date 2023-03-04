mergeInto(LibraryManager.library, {
  millis: function() {
    return Math.floor(performance.now());
  }
});

type searchStatus =
  | None
  | Loading
  | Error
  | Loaded(string);

type state = {
  searchTerm: string,
  searchStatus,
};

type action =
  | EditSearchTerm(string)
  | SearchWeather
  | SearchSucceeded(string)
  | SearchFailed;

let component = ReasonReact.reducerComponent("WeatherApp");

let handleClick = (_event, _self) => Js.log("clicked!");

let make = _children => {
  ...component,

  initialState: () => {searchStatus: None, searchTerm: ""},

  reducer: (action, state) =>
    switch (action) {
    | SearchWeather =>
      ReasonReact.UpdateWithSideEffects(
        {...state, searchStatus: Loading},
        (
          self => {
            let {searchTerm} = state;
            Js.Promise.(
              Fetch.fetch(
                {j|https://api.openweathermap.org/data/2.5/weather?q=$(searchTerm)&APPID=66984709363906be52c40c6c23640221|j},
              )
              |> then_(Fetch.Response.json)
              |> then_(json =>
                   Js.Promise.resolve(
                     self.send(SearchSucceeded(Js.Json.stringify(json))),
                   )
                 )
              |> catch(_err => Js.Promise.resolve(self.send(SearchFailed)))
              |> ignore
            );
          }
        ),
      )
    | SearchFailed => ReasonReact.Update({...state, searchStatus: Error})
    | SearchSucceeded(result) =>
      ReasonReact.Update({...state, searchStatus: Loaded(result)})
    | EditSearchTerm(term) =>
      ReasonReact.Update({...state, searchTerm: term, searchStatus: None})
    },

  render: ({state, send, handle}) => {
    let {searchTerm, searchStatus} = state;

    let message =
      if (String.length(searchTerm) === 0) {
        "Type a location in the input field...";
      } else {
        "You are searching for: \""
        ++ searchTerm
        ++ "\", such a wonderful location!";
      };

    <main className="container">
      <br />
      <h1> {ReasonReact.string("WeatherApp")} </h1>
      <h3> {ReasonReact.string(message)} </h3>
      <input
        value=searchTerm
        onChange={
          event =>
            send(EditSearchTerm(ReactEvent.Form.target(event)##value))
        }
        onKeyDown={
          event =>
            if (ReactEvent.Keyboard.keyCode(event) === 13) {
              ReactEvent.Keyboard.preventDefault(event);
              send(SearchWeather);
            }
        }
      />
      <button onClick={_event => send(SearchWeather)}>
        {ReasonReact.string("Search")}
      </button>
      <div>
        {
          switch (searchStatus) {
          | None => ReasonReact.string("Nothing here yet...")
          | Loading => ReasonReact.string("Loading...")
          | Error => ReasonReact.string("Something went wrong!")
          | Loaded(result) =>
            <pre> <code> {ReasonReact.string(result)} </code> </pre>
          }
        }
      </div>
    </main>;
  },
};
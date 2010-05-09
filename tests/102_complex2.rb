require 'test/unit'
require 'rbclips'

class Test_Complex1 < Test::Unit::TestCase

  include Clips
  include Base

  def teardown
    Base.reset
  end

  def make_older(human)
    human.age = human.age + 1
    human.birthday = "yesterday"
    assert human.save
  end

  def test_main
    # Constraints
    number = Constraint.new :type => :integer

    # Templates
    human = Template.new("human") do |t|
      t.slot :name
      t.slot :birthday
      t.slot :age, :constraint => number
    end
    assert human.save

    # Rules
    birthdays = Rule.new :birthday do |r|
      h = r.pattern human, :birthday => "today"
      r.rcall self, :make_older, h
    end
    assert birthdays.save

    # Facts
    assert Fact.new(human, :name => "Honza", :age => 22, :birthday => "yesterday").save
    assert Fact.new(human, :name => "Martin", :age => 22, :birthday => "today").save

    # Run
    Base.run

    # Chech
    Fact.all.each do |human|
      case human.name
        when "Honza" then assert_equal human.age, 22
        when "Martin" then assert_equal human.age, 23
      end
    end
  end
end
